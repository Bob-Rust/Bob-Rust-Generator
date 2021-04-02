var borst_generator = require('./x64/Release/BorstPrimitive');
var fs = require('fs');

console.log(borst_generator);
console.log();

var ALPHAS = [ 9, 18, 72, 136, 218, 255 ];
var SIZES = [ 1, 2, 4, 6, 10, 13 ];
var COLORS = [
	[ 46, 204, 112 ],
	[ 22, 161, 132 ],
	[ 52, 153, 218 ],
	[ 241, 195, 16 ],
	[ 143, 69, 173 ],
	[ 153, 163, 162 ],
	[ 52, 73, 93 ],
	[ 46, 158, 135 ],
	[ 30, 224, 24 ],
	[ 176, 122, 195 ],
	[ 231, 127, 33 ],
	[ 236, 240, 241 ],
	[ 38, 174, 96 ],
	[ 33, 203, 241 ],
	[ 126, 77, 41 ],
	[ 239, 68, 49 ],
	[ 74, 212, 188 ],
	[ 69, 48, 33 ],
	[ 49, 49, 49 ],
	[ 1, 2, 1 ]
];

function dth(value) {
    var hex = Number(value).toString(16);
    if(hex.length != 2) return '0' + hex;
	return (hex.length == 2) ? hex:('0' + hex);
}

function generate_svg(list) {
	let width = 512;
	let height = 512;
	let bg = '#000000';
	let scale = 1;
	let string;
	string = `<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="${width}" height="${height}">\n`
	       + `<rect x="0" y="0" width="${width}" height="${height}" fill="${bg}"/>\n`
		   + `<g transform="scale(${scale}) translate(0.5 0.5)">\n`;
	
	for(let i in list) {
		let shape = list[i];
		let COL = COLORS[shape.color];
		let alpha = ALPHAS[2] / 255.0;
		let col = '#' + dth(COL[0]) + dth(COL[1]) + dth(COL[2]);
		let cx = shape.x;
		let cy = shape.y;
		let cs = SIZES[shape.size];
		
		let attrs = `fill="${col}" fill-opacity="${alpha}"`;
		let seril = `<ellipse ${attrs} cx="${cx}" cy="${cy}" rx="${cs}" ry="${cs}"/>\n`;
		string += seril;
	}
	
	string += '</g>\n</svg>\n';
	
	fs.writeFile('output.svg', string, (err) => {
		if(err) return console.log(err);
		console.log('Successful!');
	});
	
	return string;
}

function update_done(list) {
	console.log('Generator is done!');
	generate_svg(list);
}

function get_path(path) {
	return 'C:/Users/Admin/Desktop/TextureRust/old_cpp/Bob-Rust-Primitive-master/examples/' + path;
}

var gen = borst_generator.generate({
	"path": get_path('ghosttest_512.png'),
	"count": 4000,
	"intervall": 100,
	"alpha": 2,
	"background": [ 235, 234, 190 ]
}, (list) => {
	console.log('Batched data: ');
}, update_done);

// console.log('Value: ' + gen);