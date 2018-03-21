import datetime
import asyncio
import urllib.parse
import sys, json, os
import copy

portDict = {'Alford':18485,'Ball':18486,'Hamilton':18487,'Holiday':18488,'Welsh':18489}

oneWayComm = {
    'Alford':['Hamilton','Welsh'],
    'Ball':['Holiday','Welsh'],
    'Hamilton':['Holiday'],
    'Welsh':[],
    'Holiday':[]
}

twoWayComm=copy.deepcopy(oneWayComm)
for sender,receivers in oneWayComm.items():
    for rec in receivers:
        twoWayComm[rec].append(sender)

class FileRW:
    def __init__(self,name):
        self.name=name

    def write(self,message):
        print("Writing",message,end='')
        with open(self.name+".txt", "a") as file:
            file.write(message)

    def most_recent_message(self,clientID):
        found=False
        time=0
        res=''
        if(os.path.isfile(self.name+'.txt')):
            file = open(self.name+'.txt', 'r')
            for line in file:
                args=line.split()
                if(len(args)<2):
                    continue
                if(args[0]=='Data' and args[-3]==clientID and float(args[-1]) > time):
                    found=True
                    res=' '.join(args[1:])
                    time=float(args[-1])
            file.close()
        return (time,res) if found else None

class MessageParser:
    def __init__(self,name):
        self.name=name

    def parse(self,message):
        try:
            command=message.split()[0]
            if(command=='IAMAT'):
                return self.IAMAT(message)
            elif(command=='WHATSAT'):
                return self.WHATSAT(message)
            elif(command=="AT"):
                return ('AT',message)
            elif(command=='ISSERVER'):
                name=message.split()[1]
                return ('ISSERVER',name)
            else:
                return None
        except:
            return None

    def IAMAT(self,message):
        clientID,latlng,time=message.split()[1:]
        #check latlng
        assert latlng[0]=='+' or latlng[0]=='-'
        index=max(latlng[1:].find('-'), latlng[1:].find('+'))+1
        assert(index!=0)
        float(latlng[:index])
        float(latlng[index:])

        #check clientTime
        assert float(time) >= 0
        clientTime=datetime.datetime.utcfromtimestamp(float(time))
        currTime=datetime.datetime.utcnow()
        timeDiff=(currTime-clientTime).total_seconds()

        #response
        sign = '+' if timeDiff > 0 else ''
        res="AT {} {}{} {} {} {}\n".format(self.name, sign, timeDiff, clientID,latlng,time)
        return ('IAMAT',res)

    def WHATSAT(self,message):
        _,clientID,radius,infoBound=message.split()
        #check infoBound
        infoBound=int(infoBound)
        #check radius
        assert float(radius) <= 50 and float(radius) >= 0
        assert infoBound <=20 and infoBound >= 0
        return ('WHATSAT',clientID,radius,infoBound)


class ProxyHerdServer:
    def __init__(self,name):
        global oneWayComm,portDict,twoWayComm
        self.oneWayCommServers=oneWayComm[name]
        self.twoWayCommServers=twoWayComm[name]
        self.portDict=portDict
        self.name=name
        self.port=portDict[name]
        self.loop=asyncio.get_event_loop()
        self.connected={name:False for name in self.oneWayCommServers}
        self.filer=FileRW(name)
        self.msgparser=MessageParser(name)
        self.writers={}

    def start(self):
        manageServer = asyncio.start_server(self.handle_message, '127.0.0.1', self.port, loop=self.loop)

        for server in self.oneWayCommServers:
            asyncio.ensure_future(self.connect_to_server(server))
        self.server = self.loop.run_until_complete(manageServer)
        self.loop.run_forever()
        self.loop.close()

    async def connect_to_server(self,other):
        while True:
            #connect
            reader=''
            writer =''
            print(self.name,'starting to connect to',other)
            while not self.connected[other]:
                try:
                    reader, writer = await asyncio.open_connection('127.0.0.1', self.portDict[other],
                                                                        loop=self.loop)
                    self.connected[other]=True
                except:
                    #nothing is listening, so sleep for a bit
                    await asyncio.sleep(3)

            self.filer.write("{} connected to {}\n".format(self.name,other))
            self.writers[other]=writer
            writer.write(('ISSERVER {}\n'.format(self.name)).encode())
            await writer.drain()

            #read from connection
            while True:
                data = await reader.readline()
                if(data == b''):
                    print(self.name,'received EOF from',other)
                    self.connected[other]=False
                    break
                print(self.name,"received message",data)
                self.flood_message(sender,data.decode())

    async def send_message(self,message,server):
        if(server not in self.connected or not self.connected[server]):
            #check if connected
            await asyncio.sleep(3)
            if(server not in self.connected or not self.connected[server]):
                #give up
                self.filer.write("{} failed to give info to {}\n".format(self.name,server))
                return


        try:
            self.writers[server].write(message.encode())
            await self.writers[server].drain()
        except:
            #server was dropped
            self.connected[server]=False
            print(self.name,"failed to write to",server)
            self.filer.write("{} failed to connect to {}\n".format(self.name,server))


    def flood_message(self,sender,message):
        clientID=message.split()[-3]
        clientInfo=self.filer.most_recent_message(clientID)
        #unique
        if(not clientInfo or clientInfo[0]<float(message.split()[-1])):
            print("M is",message)
            self.filer.write("Data {}".format(message))
            for server in self.twoWayCommServers:
                if(server!=sender):
                    asyncio.ensure_future(self.send_message(message,server))

    async def google_places(self,clientID,radius,infoBound,clientInfo):
        latlng=clientInfo.split()[-2]
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

        lines=lines[lines.decode().find('{'):]
        dic=json.loads(lines)

        dic['results']=dic['results'][:infoBound]
        result=json.dumps(dic,indent=1)
        return result

    async def handle_message(self, reader, writer):
        isServer=False
        #save server if is server
        server=''

        while True:
            data = await reader.readline()
            message = data.decode()
            if(message==''):
                #broken
                return
            result=self.msgparser.parse(message)
            response=''

            if(isServer):
                print(self.name,"received",message,"from server",server)
            if(result==None):
                response="? {}".format(message)
            elif(result[0]=='IAMAT'):
                response=result[1]
                self.flood_message(server,response)
            elif(result[0]=='WHATSAT'):
                clientID,radius,infoBound=result[1:]

                clientInfo=self.filer.most_recent_message(clientID)
                if(not clientInfo):
                    #try to wait for propagation
                    await asyncio.sleep(3)
                clientInfo=self.filer.most_recent_message(clientID)
                if(not clientInfo):
                    print(self.name,"failed to find",clientID)
                    #can't find
                    response="? {}\n".format(message)
                else:
                    google_res=await self.google_places(clientID,radius,infoBound,clientInfo[1])
                    response="{}\n {}\n".format(clientInfo[1],google_res)

            elif(result[0]=="AT"):
                isServer=True
                self.flood_message(server,message)
            elif(result[0]=='ISSERVER'):
                isServer=True
                server=result[1]
                if(server in self.connected and self.connected[server]):
                    #unnecessary, already connected
                    return
                self.writers[server]=writer
                self.connected[server]=True
                self.filer.write("{} connected to {}\n".format(self.name,server))

            if(not isServer):
                #close connection if not from server
                writer.write(response.encode())
                await writer.drain()
                writer.close()
                self.filer.write("Received: {}\n".format(message))
                self.filer.write("Responded: {}".format(response))
                return

def print_usage():
    print("Usage: Python3.6 server.py [SERVER_NAME]")
    print("Valid names:",', '.join(portDict.keys()))

def main():
    if(len(sys.argv)!=2 or sys.argv[1] not in portDict.keys()):
        print_usage()
        exit(1)
    name = sys.argv[1]
    server = ProxyHerdServer(name)
    server.start()

if __name__== "__main__":
    main()