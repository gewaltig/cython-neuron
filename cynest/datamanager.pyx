import re

cdef bytes invalid_cmd = "<+*-_Invalid Command_-*+>".encode('UTF-8')
cdef bytes composed_protected_cmd = "<+*-_Composed Protected Command_-*+>".encode('UTF-8')
cdef bytes composed_unprotected_cmd = "<+*-_Composed Unprotected Command_-*+>".encode('UTF-8')

cdef class SLIDataContainer:
    cdef classes.NESTEngine *nest_engine
    cdef commands

    def __cinit__(self):
        self.commands = {}

    cdef initialize(self, classes.NESTEngine *nest):
        self.nest_engine = nest

    cdef PyToken generate_func_pytoken(self, bytes cmd):
        cdef PyToken t = PyToken()

        self.nest_engine.run('/' + cmd + ' lookup')
        result = self.nest_engine.pop()

        if result is True:
            t.thisptr = self.nest_engine.pop_token()

        return t

    cdef PyToken generate_arg_pytoken(self, bytes cmd):
        cdef PyToken t = PyToken()

        self.nest_engine.run("/ajvehwlksjdbjds " + cmd + " def")
        self.nest_engine.run("/ajvehwlksjdbjds lookup")

        if self.nest_engine.pop():
            t.thisptr= self.nest_engine.pop_token()

        return t
        

    cdef bint add_command(self, bytes cmd):
        cdef PyToken token
        if not self.commands.has_key(cmd):
            token = self.generate_func_pytoken(cmd)
            if token.thisptr:
                self.commands[cmd] = token
                return True
            else:
                return False
        else:
            return True


    cdef PyToken get_pytoken(self, bytes cmd):
        if self.commands.has_key(cmd):
            return self.commands[cmd]
        else:
            return None

    cdef run(self, bytes cmd):
        cdef bytes command
        cdef PyToken t
        cdef composed_cmd

        m = re.search('^{ (.+?) } runprotected$', cmd)
        if hasattr(m, 'group'):
            command = m.group(1).encode('UTF-8')
            composed_cmd = composed_protected_cmd
        else:
            command = cmd
            composed_cmd = composed_unprotected_cmd

        if re.match('^[^ /]+$', command):
            if self.add_command(command):
                t = self.commands[command]
                return self.nest_engine.run_token(t.thisptr[0])
            else:
                return invalid_cmd
        else:
            return composed_cmd



# This class contains the special functions needed by
# the cython_neuron in order to access the Time and Scheduler classes
# Note that other methods having nothing to do with that are present.
# modelsFolder has been put into that class otherwise it's not persistent 
# during the execution.
cdef class TimeScheduler:
    cdef classes.TimeScheduler *thisptr

    def __cinit__(self):
        self.thisptr= new classes.TimeScheduler()
        
    def __dealloc__(self):
        del self.thisptr

    cdef double get_ms(self, int arg1, long arg2, double arg3):
        return self.thisptr.get_ms(arg1, arg2, arg3)

    cdef long get_tics_or_steps(self, int arg1, int arg2, long arg3, double arg4):
        return self.thisptr.get_tics_or_steps(arg1, arg2, arg3, arg4)

    cdef unsigned int get_scheduler_value(self, int arg1, unsigned int arg2):
        return self.thisptr.get_scheduler_value(arg1, arg2)
        
    def get_ms_on_resolution(self):
        return self.get_ms(0, -1, -1)

    def get_ms_on_tics(self, tics):
        return self.get_ms(1, tics, -1)

    def get_ms_on_steps(self, steps):
        return self.get_ms(2, steps, -1)

    def get_tics_on_resolution(self):
        return self.get_tics_or_steps(0, 1, -1, -1)

    def get_tics_on_steps(self, steps):
        return self.get_tics_or_steps(2, 1, steps, -1)

    def get_tics_on_ms(self, ms):
        return self.get_tics_or_steps(3, 1, -1, ms)

    def get_tics_on_ms_stamp(self, ms_stamp):
        return self.get_tics_or_steps(4, 1, -1, ms_stamp)

    def get_steps_on_resolution(self):
        return self.get_tics_or_steps(0, 2, -1, -1)

    def get_steps_on_tics(self, tics):
        return self.get_tics_or_steps(1, 2, tics, -1)

    def get_steps_on_ms(self, ms):
        return self.get_tics_or_steps(3, 2, -1, ms)

    def get_steps_on_ms_stamp(self, ms_stamp):
        return self.get_tics_or_steps(4, 2, -1, ms_stamp)

    def get_modulo(self, value):
        return self.get_scheduler_value(0, value)

    def get_slice_modulo(self, value):
        return self.get_scheduler_value(1, value)

    def get_min_delay(self):
        return self.get_scheduler_value(2, -1)

    def get_max_delay(self):
        return self.get_scheduler_value(3, -1)

