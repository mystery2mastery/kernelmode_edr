# kernelmode_edr (STILL IN PROGRESS)
A poc kernel mode EDR in Windows. It contains a kernel driver and a user mode client service. As of now, the user mode client does nothing. 

#### Features to be implemented:

- [x] Log Process Creation
- [x] Log Thread Creation
- [x] Log Image Loads
- [ ] Log Registry changes
- [ ] Log File changes
- [ ] Log handle creation/duplication (useful for restricting access to critical processes such as lsass.exe)
- [ ] Inject a dll containing usermode hooks into **EVERY** process from kernel space using Kernel APC. (To get info about invoked user mode API)
  - [ ] Make use of KnownDll mechanism to inject into **EVERY** process. In reality you don't need to inject into every process. This is just POC.
- [ ] Detect direct syscalls
- [ ] Detect indirect syscalls
- [ ] Detect call stack spoofing
- [ ] A logging utility (As of now logged events can be seen through DebugView tool)
- [ ] a lot more ....
