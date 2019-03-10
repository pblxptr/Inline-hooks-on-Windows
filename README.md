# simple-inline-hooks
Inline hooks on some winsock's library funcions. 
It was tested on Windows 7 x86.

**Injector** - the program which creates target process(SimpleWS) then loads to it apropriate dll (hook.dll). 

**SimpleWS** - very basic (base functionality, no error checking) tcp server, which creates socket, bind it on 3490 port, and then listens 
              for connection. Affter the client is connected it sends to it 'Hello World' message, and waits in loop for any data.

**Hook** - the dll that consists of functions which are the hooks on winsock's functions.

           Hooks are created on following functions: 
           
           - socket,
           
           - accept,
           
           - listen, 
           
           - bind, 
           
           - send, 
           
           - recv, 
           
           - connect 
         
Hooking scheme (dll responsibility):

1 - create trampoline, which is simple jump into-our controlled function.
    The function must have apropriate address so example trampoline may be defined as:
    mov eax, 0xDEADC0DE <- when loading library, this address will be replaced with &tramp_func
    jmp eax 

2 - in trampoline do what you need. For example process arguments for hooked function, or prepare 
    env that the return address will be controlled by you. If you want to jump into hooked function (e.g. original socket())
    do not forget about executing instructions that was overrided.
    
3 - return to calle.
         
All programs including dll are in very basic for, so if any error occurs, you must debug it by yourself :) 
Enjoy :) 
