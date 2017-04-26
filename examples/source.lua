

local msg = {}

local math = require("math")
local fdopen = require "posix.stdio".fdopen
local STDOUT_FILENO = require "posix.unistd".STDOUT_FILENO

local host, port = "127.0.0.1", 100
local socket = require("socket")
local tcp = assert(socket.tcp())


-- **************************************************************************

ctrl = os.fdopen(3, "w")

ctrl.write("INIT\n")
sockets = wye.parse_outputs(sys.arg[1])
ctrl.write("RUNNING\n")
ctrl.flush()
sys.stderr.write("Sender is running.\n")

-- **************************************************************************

tcp:connect(host, port)
--note the newline below
--tcp:send("hello world\n");

while true do
    time.sleep(1)

--    msg = { "x": math.random(0, 10) + 1, "y": math.random(0, 10) + 1 }
--    sys.stderr.write("Source: %s\n" % json.dumps(msg))
    for s in sockets["output"]:
        tcp:send(json.dumps(msg));
end

tcp:close

