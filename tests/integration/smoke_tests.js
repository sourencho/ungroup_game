/* Dumbest possible proof of concept smoke test.
 * Tests that a server and client can be launched and that the
 * client registers successfully. These tests should be ran from src/:
 * node tests/integration/smoke_tests.js */
var spawn = require('child_process').spawn;
var server = spawn('./build/src/server/ug-server');
var client = spawn('./build/src/client/ug-client');

DESIRED_OUTPUT = "Received client registration. Issued client ID 0"
success = false;

server.stdout.on('data', function(data) {
    var str = data.toString()
    if (str.includes(DESIRED_OUTPUT)) {
        success = true;
    }
});

// We should have successfully registered the client 1 seconds in
function testFailed() {
    server.kill();
    client.kill();
    if (success) {
        console.log("Smoke test passed!");
        process.exit(0);
    }
    console.log("Smoke test failed!");
    process.exit(1);
}

setTimeout(testFailed, 1000);
