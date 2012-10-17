from ccdb.cmd import ConsoleUtilBase
from ccdb.cmd.themes import Theme

import logging
log = logging.getLogger("ccdb.cmd.utils.pwd")


#ccdbcmd module interface
def create_util_instance():
    log.debug("      registering PrintWorkDir")
    return PrintWorkDir()


#*********************************************************************
#   Class PrintWorkDir - Prints working directory                    *
#                                                                    *
#*********************************************************************
class PrintWorkDir(ConsoleUtilBase):
    """ Prints working directory """
    
    # ccdb utility class descr part 
    #------------------------------
    command = "pwd"
    name = "PrintWorkDir"
    short_descr = "Prints working directory"
    
    def print_help(ChangeDir):
        print """ Prints working directory """ 
    # ---- end of print_help() ----


    def process(self, args):
        log.debug("  PrintWorkDir is gained a control over the process.")
        log.debug("    ".join(args))
        assert self.context is not None
        print self.context.current_path