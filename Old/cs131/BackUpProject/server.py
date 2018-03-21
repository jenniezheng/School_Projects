import datetime
import asyncio
import urllib.parse
import sys, json, os


identityArr = ['Alford','Ball','Hamilton','Holiday','Welsh']
portDict = {'Alford':8010,'Ball':8011,'Hamilton':8012,'Holiday':8013,'Welsh':8014}

communicationDict = {
    'Alford':['Hamilton','Welsh'],
    'Ball':['Holiday','Welsh'],
    'Hamilton':['Holiday','Alford'],
    'Welsh':['Alford','Ball'],
    'Holiday':['Ball','Hamilton']
}

class ProxyHerdServer:
    def __init__(self, name):
        global identityArr,portDict,communicationDict
        port=portDict[name]
        otherNames=communicationDict[name]
        otherPorts=[portDict[otherName] for otherName in communicationDict[name] ]
        nameDict= {v: k for k, v in portDict.items()}
        self.loop =  asyncio.get_event_loop()
        self.port=port
        self.otherPorts=otherPorts
        #self.connected={v: False for v in self.otherPorts}
        self.writers={}
        self.nameDict=nameDict
        self.name=name


    def start(self):
        for port in self.otherPorts:
            asyncio.run_coroutine_threadsafe(self.connect_to_other_port(port),self.loop)

        coro = asyncio.start_server(self.handle_message, 'localhost', self.port, loop=self.loop)
        self.server = self.loop.run_until_complete(coro)
            # Serve requests until Ctrl+C is pressed
        print('Serving on {}'.format(self.server.sockets[0].getsockname()))

        try:
            self.loop.run_forever()
        except:
            self.server.close()
            self.loop.run_until_complete(self.server.wait_closed())
            self.loop.end()

    @asyncio.coroutine
    def connect_to_other_port(self,port):
        reader, writer = yield from asyncio.open_connection('localhost', port,
                                                                loop=self.loop)
        #self.connected[port]=True
        with open(self.name+".txt", "a") as file:
            file.write("{} connected to {}\n".format(self.name,self.nameDict[port]))
        self.writers[port]=writer

    @asyncio.coroutine
    def send_message(self,message,otherPort):
        """
        print(message,"sending message to other ports",self.connected)

        if(not self.connected[otherPort]):
            #check if connected
            yield from asyncio.sleep(5)
            if(not self.connected[otherPort]):
                #give up
                print("Dropping info to ",self.nameDict[otherPort])
                return
        """
        print(self.name,'sending message to ',self.nameDict[otherPort])
        try:
            self.writers[otherPort].write(message.encode())
        except:
            #self.connected[otherPort]=False
            #server was dropped
            with open(self.name+".txt", "a") as file:
                file.write("{} failed to connect to {}\n".format(self.name,self.nameDict[port]))
            self.connect_to_other_port(otherPort)
            self.writers[otherPort].write(message.encode())


    def find_most_recent_message(self,clientID):
        prevTime=0
        prevRes=''
        if(os.path.isfile(self.name+'.txt')):
            file = open(self.name+'.txt', 'r')
            for line in file:
                args=line.split()
                if(args[0]=='Data' and args[1]==clientID and float(args[-1]) > prevTime):
                    prevRes=' '.join(args[2:])
                    prevTime=float(args[-1])

            file.close()
        return (prevTime,prevRes)

    def spread_message(self, message):
        print(self.name,'spreading message')
        clientID=message.split()[3]
        time=float(message.split()[-1])
        prevTime, res=self.find_most_recent_message(clientID)

        #check if own data is up to date

        if(prevTime >= time):
            print(message, "is useless, earlier message. Most recent is ",res)
            return

        else:
            res=message[9:]
            with open(self.name+".txt", "a") as file:
                file.write("Data {} {}\n".format(clientID, res))

            for otherPort in self.otherPorts:
                print(self.name,"about to send msg")
                asyncio.run_coroutine_threadsafe(self.send_message(message,otherPort),self.loop)


    @asyncio.coroutine
    def handle_message(self, reader, writer):
        data = yield from reader.read(100)
        message = data.decode()
        addr = writer.get_extra_info('peername')


        print(self.name,'received message',message)
        res=''
    #try:
        command=message.split()[0]

        #first possible command
        if(command=='IAMAT'):
            clientID,latlng,time=message.split()[1:]

            #check latlng
            assert latlng[0]=='+' or latlng[0]=='-'
            index=max(latlng[1:].find('-'), latlng[1:].find('+'))+1
            assert(index!=0)
            float(latlng[:index])
            float(latlng[index:])

            #check clientTime
            clientTime=datetime.datetime.utcfromtimestamp(float(time))
            currTime=datetime.datetime.utcnow()
            timeDiff=(currTime-clientTime).total_seconds()

            #response
            sign = '+' if timeDiff > 0 else ''
            res="AT {} {}{} {} {} {}".format(self.name, sign, timeDiff, clientID,latlng,time)

            #spread to others
            self.spread_message("SERVERMSG"+res)


        elif(command=="WHATSAT"):
            radius=message.split()[2]
            infoBound=int(message.split()[3])

            clientID=message.split()[1]
            _,otherClientInfo=self.find_most_recent_message(clientID)

            if(otherClientInfo==''):
                #try to wait for propagation
                yield from asyncio.sleep(3)

            _,otherClientInfo=self.find_most_recent_message(clientID)

            assert otherClientInfo!=''

            latlng=otherClientInfo.split()[-2]
            index=max(latlng[1:].find('-'), latlng[1:].find('+'))+1
            latlngstr=latlng[:index]+','+latlng[index:]

            key = 'AIzaSyBLb-TWOXTNMqa1Carx5XJG8U8fM6q6P3I'
            url="https://maps.googleapis.com/maps/api/place/nearbysearch/json?key={}&location={}&radius={}".format(key,latlngstr,radius)
            url = urllib.parse.urlsplit(url)

            reader, writer = yield from asyncio.open_connection(url.hostname, 443, ssl=True)
            query = ('GET {path}?{query} HTTP/1.0\r\n'
                     'Host: {hostname}\r\n'
                     '\r\n').format(path=url.path,query=url.query,hostname=url.hostname)

            writer.write(query.encode('latin1'))
            lines= yield from reader.read()
            writer.close()

            lines=lines.decode()
            lines=lines[lines.find('{'):]
            d=json.loads(lines)

            d['results']=d['results'][:infoBound]
            result=json.dumps(d,indent=1)
            res=otherClientInfo +'\n' + result+'\n'


        elif(command=="SERVERMSGAT"):
            #don't respond
            res='NONE'
            #spread to others
            self.spread_message(message)
        else:

            raise


    #except:
        #error
        #print("Unexpected error:", sys.exc_info())
    #    res="? {}".format(message)

    #finally:
        if(res!='NONE'):
            print(res)
            writer.write(res.encode())
            yield from writer.drain()
            writer.close()

            with open(self.name+".txt", "a") as file:
                file.write("Received: {}\n".format(message))
                file.write("Responded: {}\n".format(res))

def main():
    name = sys.argv[1]
    server = ProxyHerdServer(name)
    server.start()

if __name__== "__main__":
    main()