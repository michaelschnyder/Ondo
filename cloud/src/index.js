const express = require('express')
const app = express()
var createError = require('http-errors')
const port = process.env.PORT || 3000

require('dotenv').config()

var indexRouter = require('./routes/index.routes');
app.use('/', indexRouter);

app.use(function (req, res, next) {
  next(createError(404));
});

app.listen(port, () => console.log(`Example app listening on port ${port}!`))

module.exports = app

