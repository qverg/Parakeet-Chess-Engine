from pexpect import popen_spawn

class Engine:
    def __init__(self, file_path: str, starting_pos_fen: str = None):
        self.analyzer = popen_spawn.PopenSpawn(file_path, encoding="utf-8")
        self.analyzer.expect(".+")
        print(self.analyzer.after)

    def reset(self):
        print("Sending $reset")
        self.analyzer.sendline("$reset")
        print(self.get_engine_output())

        self.analyzer.sendline("12")
        print(self.get_engine_output())

    def close(self):
        print("Sending $quit")
        self.analyzer.sendline("$quit")

    def get_engine_output(self):
        self.analyzer.expect(".+")
        lines = self.analyzer.after.split("\n")
        output = ""
        for line in lines:
            if line[0] != "$":
                output += line + "\n"
        return output[:-1]
        
