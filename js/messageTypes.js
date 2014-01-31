function define(name, value) {
    Object.defineProperty(exports, name, {
        value:      value,
        enumerable: true
    });
}

define("PRESENTATION",0);
define("SET_VARIABLE",1);
define("REQ_VARIABLE",2);
define("ACK_VARIABLE",3);
define("INTERNAL",4);

