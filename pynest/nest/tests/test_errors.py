"""
Tests for error handling
"""

import unittest
import nest
import sys

class ErrorTestCase(unittest.TestCase):
    """Tests if errors are handled correctly"""

    def test_Raise(self):
        """Error raising"""

        nest.ResetKernel()
        try:
            raise nest.NESTError('test')
            self.fail('an error should have risen!')  # should not be reached
        except nest.NESTError:
            info = sys.exc_info()[1]
            if not "test" in info.__str__():
                self.fail('could not pass error message to NEST!')             
        # another error has been thrown, this is wrong
        except: 
          self.fail('wrong error has been thrown')


    def test_StackUnderFlow(self):
        """Stack underflow"""

        nest.ResetKernel()
        try:
            nest.sr('clear ;')
            self.fail('an error should have risen!') # should not be reached
        except nest.NESTError:
            info = sys.exc_info()[1]
            if not "StackUnderflow" in info.__str__():
                self.fail('wrong error message')              
        # another error has been thrown, this is wrong
        except: 
          self.fail('wrong error has been thrown')  


    def test_DivisionByZero(self):
        """Division by zero"""

        nest.ResetKernel()
        try:
            nest.sr('1 0 div')
            self.fail('an error should have risen!')  # should not be reached
        except nest.NESTError:
            info = sys.exc_info()[1]
            if not "DivisionByZero" in info.__str__():
                self.fail('wrong error message')              
        # another error has been thrown, this is wrong
        except: 
          self.fail('wrong error has been thrown')  


    def test_UnknownNode(self):
        """Unknown node"""

        nest.ResetKernel()
        try:
            nest.Connect([99],[99])
            self.fail('an error should have risen!')  # should not be reached
        except nest.NESTError:
            info = sys.exc_info()[1]
            if not "UnknownNode" in info.__str__():
                self.fail('wrong error message')              
        # another error has been thrown, this is wrong
        except: 
          self.fail('wrong error has been thrown')

          
    def test_UnknownModel(self):
        """Unknown model name"""

        nest.ResetKernel()
        try:
            nest.Create(-1)
            self.fail('an error should have risen!')  # should not be reached
        except nest.NESTError:
            info = sys.exc_info()[1]
            if not "UnknownModelName" in info.__str__():
                self.fail('wrong error message')              
        # another error has been thrown, this is wrong
        except: 
          self.fail('wrong error has been thrown')  


def suite():

    suite = unittest.makeSuite(ErrorTestCase,'test')
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
