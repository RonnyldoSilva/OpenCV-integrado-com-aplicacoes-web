var express = require('express');
//multer é para 'manipular' arquivos
var multer = require('multer');
var upload = multer({dest: './uploads/'});
var path = require('path');
var uuid = require('uuid');
var net = require('net');
var app = express();

app.use(express.static('public'));

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});

app.post('/send_photo', upload.single('photo'), function(req, res){
	var output_image = uuid() + '.png';
	var absolute_path = path.resolve('./uploads/' + req.file.filename);
	var output_path = path.resolve('./uploads_output/' + output_image);	
	var type=req.body.type;
	
	var socket = new net.Socket();
	socket.connect(9000, 'localhost', function(){
		socket.write(absolute_path + ',' + output_path + ',' +type)
	});
	var response;
	socket.on('data', function(data){
		response = data;
	});
	socket.on('close', function(){
		res.json({response: parseInt(response[0]), output: output_image})
		//res.send(reponse);
	});	
});

app.get('/photo/:name', function(req, res){
	res.sendFile(path.resolve('./uploads_output/' + req.params.name));
});

app.listen(8080);
