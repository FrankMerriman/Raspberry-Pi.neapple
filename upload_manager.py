from subprocess import Popen
import sys

filename = 'drive_uploader.py'
while True:
    print("Starting " + filename)
    p = Popen("python " + filename, shell=True)
    p.wait()
