## **C**ontent { **HI**gh **P**erformance } **D**elivery

Chipd is a webserver for static content. It gains performance from preloading all the static files to the memory.

### Hash Algorithm : 

XOR hash Max 8 bit size range

### HTTP Parser : 

NGINX / Node's http parser https://github.com/joyent/http-parser

### ~Todo~

We have a long way to go!

 * Implement a hasher [DONE]
 * http parser [DONE]
 * loading files to memory
 * multithreading [DONE]
 * SSL
 * Support for compression
 * Security
 * Portability : atleast *nix based systems
