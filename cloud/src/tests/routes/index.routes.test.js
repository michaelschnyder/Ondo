var chai = require('chai');
var chaiHttp = require('chai-http');
var app = require('../../index');

// Configure chai
chai.use(chaiHttp);
chai.should();

describe("Index", () => {
    describe("GET Index", () => {
        it("should get index", (done) => {
            chai.request(app)
                .get('/')
                .end((err, res) => {
                    res.should.have.status(200);
                    res.body.should.be.a('object');
                    done();
                });
        });
    });
});

describe("Sensor Data", () => {
    describe("GET Sensor Data", () => {
        it('should list sensor data', (done) => {
            chai.request(app)
                .get('/sensor')
                .end((err, res) => {
                    res.should.have.status(200);
                    res.should.be.json;
                    res.body.should.have.length(1);
                    done();
                });
        });
    });
});

describe("Ac Data", () => {
    describe("GET Ac Data", () => {
        it('should list current ac settings', (done) => {
            chai.request(app)
                .get('/ac')
                .end((err, res) => {
                    res.should.have.status(200);
                    res.should.be.json;
                    res.body.should.have.length(1);
                    done();
                });
        });
    });
});

describe("Ac Data", () => {
    describe("GET Ac Data", () => {
        it('should list current ac settings', (done) => {
            chai.request(app)
                .post('/ac')
                .end((err, res) => {
                    res.should.have.status(200);
                    done();
                });
        });
    });
});
