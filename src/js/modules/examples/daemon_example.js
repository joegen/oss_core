var system = require("system");
var console = require("console");
var opt = require("getopt");
var File = require("file").File;

const STDIN_FILENO = require("constants").STDIN_FILENO;
const STDOUT_FILENO = require("constants").STDOUT_FILENO;
const STDERR_FILENO = require("constants").STDERR_FILENO;

var daemonize = function()
{
  var pid = 0;
  if (system.getppid() === 1)
  {
    return;
  }
  pid = system.fork();
  if (pid < 0)
  {
    // Fork error
    system.exit(1);
  }
  else if (pid > 0)
  {
    // Exit the parent process
    system.exit(0); 
  }
  
  // Obtain a new process group
  system.setsid();
  
  // Close I/O descriptors
  system.close(STDIN_FILENO);
  system.close(STDOUT_FILENO);
  system.close(STDERR_FILENO);
}

// Just for kicks, pretend we are in C
var main = function()
{
  if (String.fromCharCode(opt.getopt("d")) === "d")
  {
    console.log("Daemonizing...");
    daemonize();
    
    //
    // You may also call system.daemonize() which implements the same function
    //
  }
  var pid = system.getpid();
  var pidFile = new File("daemon_example.pid", "w");
  if (!pidFile.lock())
  {
    // A previous instance already has the lock
    system._exit(-1);
  }
  pidFile.write(new Buffer(pid.toString()));
  pidFile.flush();
  system.sleep(10);
  console.log("You should never see this if we are a daemon.  STDIN and STDERR are closed");
  pidFile.unlock()
  pidFile.close();
  system._exit(0);
}

main();


