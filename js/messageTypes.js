function define(name, value) {
    Object.defineProperty(exports, name, {
        value:      value,
        enumerable: true
    });
}

define("SENSOR_PRESENTATION",0);
define("SET_VARIABLE",1);
define("REQUEST_STATUS",2);
define("MESSAGE",99);

