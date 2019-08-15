const express = require('express')
const app = express()
var createError = require('http-errors')
const port = process.env.PORT || 3000
const shdlr = require('./services/scheduler');

require('dotenv').config();

var indexRouter = require('./routes/index.routes');
app.use('/', indexRouter);

app.use(function (req, res, next) {
  next(createError(404));
});

app.listen(port, function () {
  console.log(`App listening on port ${port}!`)
  shdlr.scheduledJobs();
});

module.exports = app

