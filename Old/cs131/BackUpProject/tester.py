import asyncio,sys
import proxyHerdServer

def cb():
    line=input()
    #try:
    cmd=line.split()[0]
    if cmd=='drop':
        num=int(line.split()[1])
        proxies[num].server.close()

    elif cmd=='connect':
        """
        num=int(line.split()[1])
        proxy=proxies[num]
        for port in proxy.otherPorts:
            asyncio.run_coroutine_threadsafe(proxy.connect_to_other_port(port),proxy.loop)
        coro = asyncio.start_server(proxy.handle_message, 'localhost', proxy.port, loop=proxy.loop)
        proxy.server = proxy.loop.run_until_complete(coro)
        """
    else:
        print("Cmd not recognized")
    #except:
    #    print("Exception")
    #    pass
if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    proxies = []
    for i in range(3):
        proxy=proxyHerdServer.ProxyHerdServer(i,loop)
        for port in proxy.otherPorts:
            asyncio.run_coroutine_threadsafe(proxy.connect_to_other_port(port),proxy.loop)

        coro = asyncio.start_server(proxy.handle_message, 'localhost', proxy.port, loop=proxy.loop)
        proxy.server = proxy.loop.run_until_complete(coro)

        print('Serving on {}'.format(proxy.server.sockets[0].getsockname()))
        proxies.append(proxy)

    loop.add_reader(sys.stdin, cb)

    try:
        print("Running... Press ^C to shutdown")
        loop.run_forever()
    except KeyboardInterrupt:
        pass

    for i, proxy in enumerate(proxies):
        print("Closing server {0}".format(i+1))
        proxy.server.close()
        loop.run_until_complete(proxy.server.wait_closed())


    loop.close()