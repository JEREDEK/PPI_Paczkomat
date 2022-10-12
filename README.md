Haha skuter go brrr

Serial commands:
f - Set direction to forwards (Default)
b - Set direction to backwards
p - Set power level in %. Example: "p50" sets power to 50%, (0% by default, minimum power to see a reaction is 20%, for actual movement, it's dependant on the battery charge and speed setting.)
g - Returns settings on serial
s - Start the engine with previously set power
t - Stop the engine

Serial setup:
"start;p=XXX;d=Y;t=Z;end"
Where:
XXX - Set power in %
Y - Direction, 1 is forwards and 0 is backwards
Z - Throttle state, 1 means the engine is on and 0 means it's off.
