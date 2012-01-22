
var xl = require('./build/Release/xl');

console.log("Max CPUs: "+xl.maxCPUs);

console.log("libxl version: "+xl.LIBXL_VERSION);

console.log("----- Domains -----");
console.log(xl.domains);
console.log();

console.log("---- CPU Pools ----");
console.log(xl.cpuPools);
console.log();

console.log("----- Version -----");
console.log(xl.version);
console.log();