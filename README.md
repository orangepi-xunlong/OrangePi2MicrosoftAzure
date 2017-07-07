
              How to build the environment of OrangePi for Mircrosoft Azure 

  In order to use Mircrosoft Azure on OrangePi,we should build the development environment.
  It's easy for everyone who use OrangePi to connect Azure to build your development.
  Pls download the porting package from web "www.orangepi.org/downloadresources",
  the package name is "OrangePi2Azure_PortingLib".And then we get the this package,pls 
  follow steps:
    -> Pls decompress the package and get source code.
    -> Entry the dirent of the source code.
    -> Pls run "setup.sh" to install fastly running library,as command:
	 ./setup.sh
    -> Now,we can use OrangePi2Azure API in your program and also you can refer to sample code 
       to use API in your program,as command:
	 ./build.sh
       To use "ToAzure" to connect Mircrosoft Azure,as command:
	 ./ToAzure
       To use "ToSound" to get sound,as command:
         ./ToSound
