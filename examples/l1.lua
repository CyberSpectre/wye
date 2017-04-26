#!/usr/bin/env lua

local clock = os.clock

function sleep(n)  -- seconds
   local t0 = clock()
   while clock() - t0 <= n do
   end
end

j=1
while true do
        print (j)
        j = j + 1
        sleep(1)
end
