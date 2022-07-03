from pexpect import popen_spawn


p = popen_spawn.PopenSpawn("python parakeet\\parakeet.exe", encoding="utf-8")

for i in range(10):
    p.expect(".+")
    print(p.after)