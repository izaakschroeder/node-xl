
var xl = require('./build/Release/xl');

console.log("Max CPUs: "+xl.maxCPUs);

console.log("libxl version: "+xl.LIBXL_VERSION);

console.log("----- Domains -----");
console.log(xl.domains);
console.log();

console.log("-- Domain 0 Info --");
console.log(xl.domainInfo(0));
console.log();

console.log("---- CPU Pools ----");
console.log(xl.cpuPools);
console.log();

console.log("----- Version -----");
console.log(xl.version);
console.log();

console.log("- Domain Creation -");
var id = xl.domainCreate({ });
if (id) {
	console.log("Created domain: "+id);
	console.log(xl.domainInfo(id));
	xl.domainDestroy(id);
	console.log("Destroyed.")
}
else {
	console.log("Failed to create domain.")
}
console.log();