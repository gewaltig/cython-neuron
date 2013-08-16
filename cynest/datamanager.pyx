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




cdef class Unit:
    cdef classes.UnitManager *thisptr

    def __dealloc__(self):
        del self.thisptr

cdef class tic(Unit):
    def __cinit__(self, t):
        self.thisptr= new classes.UnitManager(1, <long>t)

    def create(self, t):
        return tic(t)


cdef class step(Unit):
    def __cinit__(self, t):
        self.thisptr= new classes.UnitManager(2, <long>t)
       
    def create(self, t):
        return step(t)


cdef class ms(Unit):
    def __cinit__(self, t):
        self.thisptr= new classes.UnitManager(3, <double>t)

    def create(self, t):
        return ms(t)


cdef class ms_stamp(Unit):
    def __cinit__(self, t):
        self.thisptr= new classes.UnitManager(4, <double>t)

    def create(self, t):
        return ms_stamp(t)




cdef class Time:
    cdef classes.Time thisptr

    def __cinit__(self, Unit t):
        self.thisptr = t.thisptr[0].generateTime()
       
    cdef set(self, classes.Time t):
        self.thisptr = t

    def create(self, t):
        return Time(t)

    def get_tics(self):
        return self.thisptr.get_tics()

    def get_steps(self):
        return self.thisptr.get_steps()

    def get_ms(self):
        return self.thisptr.get_ms()

    def get_resolution(self):
        cdef classes.Time t = classes.get_resolution()
        cdef Time tm = Time(ms(0.0))
        tm.set(t)
        return tm


