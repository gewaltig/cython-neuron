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



