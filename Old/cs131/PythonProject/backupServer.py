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

connectionDict = {
    'Alford':['Hamilton','Welsh'],
    'Ball':['Holiday','Welsh'],
    'Hamilton':['Holiday'],
    'Welsh':[],
    'Holiday':[]
}

class ProxyHerdServer:
    def __init__(self, name):
        global identityArr,portDict,communicationDict,connectionDict
        port=portDict[name]
        otherNames=communicationDict[name]
        otherPorts=[portDict[otherName] for otherName in communicationDict[name] ]
        nameDict= {v: k for k, v in portDict.items()}
        self.loop =  asyncio.get_event_loop()
        self.port=port
        self.otherPorts=otherPorts
        self.connected={v: False for v in self.otherPorts}
        self.initConnects=[portDict[otherName] for otherName in connectionDict[name] ]
        self.writers={}
        self.nameDict=nameDict
        self.name=name


    def start(self):
        for port in self.initConnects:
            asyncio.run_coroutine_threadsafe(self.connect_to_other_port(port))

        coro = asyncio.start_server(self.handle_message, '127.0.0.1', self.port, loop=self.loop)
        self.server = self.loop.run_until_complete(coro)
            # Serve requests until Ctrl+C is pressed
        print('Serving on {}'.format(self.server.sockets[0].getsockname()))

        try:
            self.loop.run_forever()
        except:
            self.server.close()
            self.loop.run_until_complete(self.server.wait_closed())
            self.loop.end()

    async def connect_to_other_port(self,port):
        while True:
            print(self.name,'starting to connect to',self.nameDict[port])
            reader, writer = await asyncio.open_connection('127.0.0.1', port,
                                                                    loop=self.loop)
            print(self.name,'connected to',self.nameDict[port])
            self.connected[port]=True

            with open(self.name+".txt", "a") as file:
                file.write("{} connected to {}\n".format(self.name,self.nameDict[port]))
            self.writers[port]=writer
            writer.write(('ISSERVER {}\n'.format(self.port)).encode())
            await writer.drain()

            while True:
                data = await reader.readline()
                if(data==''):
                    print(self.name,'failed to connect while reading to',self.nameDict[port])
                    break
                print(self.name,"received message",data)
                self.spread_message(data.decode())



    def send_message(self,message,otherPort):
        if(not self.connected[otherPort]):
            #check if connected
            await asyncio.sleep(5)
            if(not self.connected[otherPort]):
                #give up
                with open(self.name+".txt", "a") as file:
                    file.write("{} failed to give info to {}\n".format(self.name,self.nameDict[otherPort]))

                print(self.name,"Failed to give info to",self.nameDict[otherPort])
                return

        try:
            self.writers[otherPort].write(message.encode())
            await writer.drain()
            print(self.writers[otherPort], 'sent!')
        except:
            self.connected[otherPort]=False
            #server was dropped
            print(self.name,'failed to connect in send_message to', self.nameDict[otherPort])

            with open(self.name+".txt", "a") as file:
                file.write("{} failed to connect to {}\n".format(self.name,self.nameDict[otherPort]))



    def find_most_recent_message(self,clientID):
        prevTime=0
        prevRes=''
        if(os.path.isfile(self.name+'.txt')):
            file = open(self.name+'.txt', 'r')
            for line in file:
                args=line.split()
                if(len(args)<2):
                    continue
                if(args[0]=='Data' and args[1]==clientID and float(args[-1]) > prevTime):
                    prevRes=' '.join(args[2:])
                    prevTime=float(args[-1])

            file.close()
        return (prevTime,prevRes)

    def spread_message(self, message):
        clientID=message.split()[3]
        time=float(message.split()[-1])
        prevTime, res=self.find_most_recent_message(clientID)
        #check if own data is up to date
        if(prevTime >= time):
            print(message, "is useless, earlier message. Most recent is ",res)
            return

        else:
            with open(self.name+".txt", "a") as file:
                file.write("Data {} {}\n".format(clientID, message))

            for otherPort in self.otherPorts:
                asyncio.run_coroutine_threadsafe(self.send_message(message,otherPort),self.loop)


    async def handle_message(self, reader, writer):
        while True:
            data = await reader.readline()
            message = data.decode()
            addr = writer.get_extra_info('peername')


            print(self.name,'received message',message)
            res=''
            try:
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
                    res="AT {} {}{} {} {} {}\n".format(self.name, sign, timeDiff, clientID,latlng,time)

                    #spread to others
                    self.spread_message(res)


                elif(command=="WHATSAT"):
                    radius=message.split()[2]
                    infoBound=int(message.split()[3])

                    clientID=message.split()[1]
                    _,otherClientInfo=self.find_most_recent_message(clientID)

                    if(otherClientInfo==''):
                        #try to wait for propagation
                        await asyncio.sleep(3)

                    _,otherClientInfo=self.find_most_recent_message(clientID)

                    assert otherClientInfo!=''

                    latlng=otherClientInfo.split()[-2]
                    index=max(latlng[1:].find('-'), latlng[1:].find('+'))+1
                    latlngstr=latlng[:index]+','+latlng[index:]

                    key = 'AIzaSyBLb-TWOXTNMqa1Carx5XJG8U8fM6q6P3I'
                    url="https://maps.googleapis.com/maps/api/place/nearbysearch/json?key={}&location={}&radius={}".format(key,latlngstr,radius)
                    url = urllib.parse.urlsplit(url)

                    reader, writer = await asyncio.open_connection(url.hostname, 443, ssl=True)
                    query = ('GET {path}?{query} HTTP/1.0\r\n'
                             'Host: {hostname}\r\n'
                             '\r\n').format(path=url.path,query=url.query,hostname=url.hostname)

                    writer.write(query.encode('latin1'))
                    lines= await reader.read()
                    writer.close()

                    lines=lines.decode()
                    lines=lines[lines.find('{'):]
                    d=json.loads(lines)

                    d['results']=d['results'][:infoBound]
                    result=json.dumps(d,indent=1)
                    res=otherClientInfo +'\n' + result+'\n'
                elif (command=="AT"):
                    print(self.name,'received at msg')
                    #don't respond
                    res='NONE'
                    #spread to others
                    self.spread_message(message)
                elif (command=='ISSERVER'):
                    port=int(message.split()[1])
                    self.writers[port]=writer
                    self.connected[port]=True

                    with open(self.name+".txt", "a") as file:
                        file.write("{} connected to {}\n".format(self.name,self.nameDict[port]))
                    print(self.name,'recieved connection to',self.nameDict[port])
                    res='NONE'

                else:
                    res="? {}".format(message)

            except:
                print("Unexpected error:", sys.exc_info())
                res="? {}".format(message)

            finally:
                if(res!='NONE'):
                    writer.write(res.encode())
                    await writer.drain()
                    writer.close()

                    with open(self.name+".txt", "a") as file:
                        file.write("Received: {}\n".format(message))
                        file.write("Responded: {}\n".format(res))

            if(res!='NONE'):
                break
def main():
    name = sys.argv[1]
    server = ProxyHerdServer(name)
    server.start()

if __name__== "__main__":
    main()