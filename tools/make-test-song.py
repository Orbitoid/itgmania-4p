#!/usr/bin/env python3
"""Generate tests/songs/QuadTest: silent ogg + .sm with 5 dance-single charts of increasing density."""
import subprocess, sys, os
out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), "..", "tests", "songs", "QuadTest")
os.makedirs(out, exist_ok=True)
BPM, MEASURES = 120, 48  # 96 s
subprocess.run(["ffmpeg","-y","-loglevel","error","-f","lavfi","-i","anullsrc=r=44100:cl=stereo","-t",str(MEASURES*2+4),"-c:a","libvorbis",os.path.join(out,"silence.ogg")],check=True)
def chart(div, name, meter):
    # div = notes per measure; one single tap per row, arrow pattern cycles
    meas=[]
    for m in range(MEASURES):
        rows=[]
        for i in range(div):
            k=m*div+i
            line=["0"]*4; line[(k*3+k//4)%4]="1"
            rows.append("".join(line))
        meas.append("\n".join(rows))
    body="\n,\n".join(meas)
    return f"#NOTES:\n     dance-single:\n     QuadTest:\n     {name}:\n     {meter}:\n     0,0,0,0,0:\n{body}\n;\n"
sm=f"""#TITLE:Quad Test;
#ARTIST:Generated;
#MUSIC:silence.ogg;
#OFFSET:0.000;
#SAMPLESTART:0.0;
#SAMPLELENGTH:10.0;
#SELECTABLE:YES;
#BPMS:0.000={BPM}.000;
"""
for div,name,meter in [(2,"Beginner",1),(4,"Easy",3),(8,"Medium",6),(12,"Hard",9),(16,"Challenge",12)]:
    sm+=chart(div,name,meter)
open(os.path.join(out,"QuadTest.sm"),"w").write(sm)
print("wrote",out)
