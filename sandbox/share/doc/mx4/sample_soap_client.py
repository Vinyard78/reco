import SOAPpy
import sys
import base64
import os





if __name__ == "__main__":

    server = SOAPpy.SOAPProxy(sys.argv[1], namespace = "http://milpix2.inrialpes.fr/mx4.wsdl")

    #server.config.dumpSOAPOut = 1
    server.config.dumpSOAPIn = 1

    print server.ping();
    
    size= os.path.getsize(sys.argv[2])

    try: 
        fd=open(sys.argv[2])
    except Exception, e:
        print "Cannot open file"
        sys.exit(1)
    
    content = base64.b64encode(fd.read())
    print "%u"%int(size)
    list = server.search( len(content), content)
#    print list
    for line in list:
        print line

