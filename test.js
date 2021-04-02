var borst_generator = require('./x64/Release/BorstPrimitive');
console.log(borst_generator);
console.log();

function update_done(list) {
	console.log('Generator is done!');
	console.log(list);
}

var gen = borst_generator.generate({
	"path": "C:/Users/Admin/Desktop/2020 Project/ScrapMod/GhidraTracer.png",
	"count": 4000,
	"intervall": 100,
	"alpha": 2,
	"background": [ 0, 0, 0 ]
}, (list) => {
	console.log('Batched data: ');
}, update_done);

// console.log('Value: ' + gen);