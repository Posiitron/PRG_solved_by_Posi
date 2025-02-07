Usage: ./prgsem [OPTIONS]
Options:
  -h, --help        Display this help message
  -n <value>        Set the value of 'n' (default: 60)
  -w <value>        Set the width (default: 640)
  -h <value>        Set the height (default: 480)
  --pipe-in <file>  Specify the input pipe file (default: /tmp/computational_module.out)
  --pipe-out <file> Specify the output pipe file (default: /tmp/computational_module.in)

IN APP CONTROL:
  press:
    'r' - reset CID for remote computation if '1'(COMP MODE) or reset the computation location if 'c'(PC MODE)
    'g' - get firmware version of the computation module
    'c' - compute and visualize julia set locally (ENTERS THE PC MODE)
    's' - set and set intiak computation conditions to computation module
    '1' - begin the module computation (ENTERS THE COMP MODE)
    'l' - clear buffer and redraw the screen
    'p' - save current screen as PNG (*BONUS)
    'j' - save current screen as JPG (*BONUS)
    'q' - escape the program - close all threads - clean exits main program
    'a' - instant abort current computation - for the '1' feature (local computation 'c' can't be aborted)
    'x' - animate constant change of the julia set (*BONUS), first press starts the animation, second stops it
    '+' - zoom in, only in PC MODE (*BONUS)
    '-' - zoom out, only in PC MODE (*BONUS)
    'arrows' - move inside the PC MODE screen (*BONUS)
  mouse (*BONUS):
    'scroll' - zoom in/out
    'pan' - pan 
