from pexpect import popen_spawn

class Engine:
    def __init__(self, file_path: str, starting_pos_fen: str = None):
        self.analyzer = popen_spawn.PopenSpawn(file_path, encoding="utf-8")
        self.analyzer.expect(".+")
        print(self.analyzer.after)

    def get_engine_output(self):
        """Returns the engine output that doesn't have $'s in front"""
        self.analyzer.expect(".+")
        lines = self.analyzer.after.split("\n")
        output = ""
        for line in lines:
            if line[0] != "$":
                output += line + "\n"
        return output[:-1]

    def reset_board(self):
        """Resets the board to the starting position"""
        print("Sending $reset")
        self.analyzer.sendline("$reset")
        print(self.get_engine_output())

        self.analyzer.sendline("12")
        print(self.get_engine_output())

    def close(self):
        """Quits the engine"""
        print("Sending $quit")
        self.analyzer.sendline("$quit")
    
    def get_position(self):
        """Returns the game position currently stored in the engine"""
        print("Sending $getposition")
        self.analyzer.sendline("$getposition")
        return self.get_engine_output()
        
