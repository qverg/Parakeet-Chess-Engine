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
            if len(line) > 0 and line[0] != "$" and line[0] != ">":
                output += line + "\n"
        return output.strip()

    def reset_board(self):
        """Resets the board to the starting position."""
        print("Sending $reset")
        self.analyzer.sendline("$reset")
        print(self.get_engine_output())

    def close(self):
        """Quits the engine. Idek if this is important but I'm doing this when the program closes."""
        print("Sending $quit")
        self.analyzer.sendline("$quit")
    
    def get_position(self):
        """Returns the game position currently stored in the engine."""
        print("Sending $getposition")
        self.analyzer.sendline("$getposition")
        return self.get_engine_output()

    def suggest_move_square(self, square: int) -> list:
        """Returns a list of moves possible from the given square."""
        print("Sending", square)
        self.analyzer.sendline(str(square))
        moves = self.get_engine_output().strip().split()
        if '$Enter' in moves:
            return []
        return moves
        
