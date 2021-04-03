var borst_generator = require('./x64/Release/BorstPrimitive');
var fs = require('fs');

console.log(borst_generator);
console.log();

var ALPHAS = borst_generator.alphas();
var COLORS = borst_generator.colors();
var SIZES = borst_generator.sizes();

function get_path(path) {
	return 'C:/Users/Admin/Desktop/TextureRust/old_cpp/Bob-Rust-Primitive-master/examples/' + path;
}

var settings = {
	path: get_path('testin.jpg'),
	count: 4000,
	interval: 500,
	alpha: 2,
	width: 512,
	height: 512,
	background: [ 0, 0, 0 ]
};

function ColorHex(c) {
	function h(value) {
		var hex = Number(value).toString(16);
		if(hex.length != 2) return '0' + hex;
		return (hex.length == 2) ? hex:('0' + hex);
	}
	return '#' + h(c[0]) + h(c[1]) + h(c[2]);
}

function write_svg(map, idx) {
	let width = map.width;
	let height = map.height;
	let bg = ColorHex(settings.background);
	let list = map.list;
	let string;
	string = `<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="${width}" height="${height}">\n`
	       + `<rect x="0" y="0" width="${width}" height="${height}" fill="${bg}"/>\n`
		   + `<g transform="scale(1) translate(0.5 0.5)">\n`;
	
	let alpha = ALPHAS[settings.alpha] / 255.0;
	for(let i in list) {
		let shape = list[i];
		let color = ColorHex(COLORS[shape.color]);
		let cx = shape.x;
		let cy = shape.y;
		let cs = SIZES[shape.size];
		
		let attr = `fill="${color}" fill-opacity="${alpha}"`;
		string += `<ellipse ${attr} cx="${cx}" cy="${cy}" rx="${cs}" ry="${cs}"/>\n`;
	}
	
	string += '</g>\n</svg>\n';
	fs.writeFile(`out/output_${idx}.svg`, string, (err) => { if(err) console.log(err); });
}

var global_index = 0;
var gen = borst_generator.generate(settings, (map) => {
	let idx = global_index;
	global_index += settings.interval;
	
	console.log('Batched data: ' + idx);
	write_svg(map, idx);
}, (map) => {
	console.log('Generator is done!');
	write_svg(map, 'done');
});