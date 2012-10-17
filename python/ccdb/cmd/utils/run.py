import logging
from ccdb.cmd import ConsoleUtilBase

log = logging.getLogger("ccdb.cmd.utils.run")

#ccdbcmd module interface
def create_util_instance():
    log.debug("      registering CurrentRun")
    return CurrentRun()


#*********************************************************************
#   Class CurrentRun - gets or sets current working run              *
#                                                                    *
#*********************************************************************
class CurrentRun(ConsoleUtilBase):
    """ gets or sets current working run """
    
    # ccdb utility class descr part 
    #------------------------------
    command = "run"
    name = "CurrentRun"
    short_descr = "gets or sets current working run"
    
    def print_help(self):
        print """ gets or sets current working run
        run (with no aguments) will display current working run
        run <run_number> will set current working run to this number
        
        """ 
    # ---- end of print_help() ----


    def process(self, args):
        log.debug("PrintWorkDir is gained a control over the process.")
        log.debug("  " + " ".join(args))
        
        assert self.context is not None

        if len(args):
            #set working run?
            try:
                self.context.current_run = int(args[0])
                log.info("Working run is %i", self.context.current_run)
            except ValueError:
                log.warning("cannot read run number")
                return 1
        else:
            #get working run
            print self.context.current_run
        
        # all is fine
        return 0