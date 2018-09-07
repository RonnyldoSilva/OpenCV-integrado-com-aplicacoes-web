var uuid = require('uuid');
var multer = require('multer');
var upload = multer({ dest: 'uploads/' });


app.post('/', upload.single('photo'), function(req, res) {

});

str = uuid();

res.sendFile(path.resolve(__dirname + './uploads'));
