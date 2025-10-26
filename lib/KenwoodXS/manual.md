Kenwood XS System Control
See XS-Connection.jpg for pinout.

Based on reverse engineering of the protocol done by Olaf Such (ol...@ihf.rwth-aachen.de).
The following excerpts were taken from de.sci.electronics posts by Olaf from 1997.

"Spezifikationen:
positive Logik, TTL Pegel, Startbit 15ms, 8Bit daten, danach geht
CTRL wieder auf 0V"

"The code consists of: CTRL goes High, 2ms later, SDAT goes High for about
15ms (StartBit), then a Low on SDAT for either approx. 15ms or 7.5ms 
followed by a High level on SDAT for 7.5ms (FrameSignal)
then Low again for 15 or 7.5ms, High for 7.5, etc. 8 times all together.
CTRL then returns to Low."
*/

/*
Here are commands for Kenwood KX-3050 cassette deck:

Commands working in both the power-on mode and stand-by mode (in decimal):
121 - play
>> 112, 113, 115, 117, 122, 123, 125 - stop

Commands working only in the power-on mode (in decimal):
66 - search next track
68 - stop
70 - play if stopped or paused, repeat current song if playing
72 - record
74 - search previous track
76 - pause
*/

/*
Here are the commands FROM the Kenwood KR-A5030 Audio/Video Receiver
These are transmitted from the Kenwood RC-5030 Remote Control Unit
(in decimal) 

MISC
Power On 181
Power Off 185 (same button as above)
Tuner BAND 123
Num Pad +10 164

INPUT
Video 1 122
Video 2 113
Tape 1 121
Tape 2 (toggle on) 20
Tape 2 (toggle off) 24
Phono 115
Tuner 123
CD 125

TAPE A
>> (fast forward) 67
[] (stop) 69
> (play) 71
* (record) 73
<< (rewind) 75
|| (pause) 77
< (rewind (slow?)) 79

TAPE B
>> 66
[] 68
> 70
* 72
<< 74
|| 76
< 78

CD
>/|| (toggle play or pause) 194
[] 198
Disc 174
Start CD 222

Below behavior needs to be further investigated but I suspect the receiver is expecting press and hold to scrub >> or << (and >>| or |<< ) and on release, the receiver sends the stop command: 
<< 169
>> 161
|<< 200
>>| 192
Stop scrubbing (release) 177