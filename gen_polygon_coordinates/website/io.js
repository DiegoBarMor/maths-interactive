// INIT GLOBALS
window.dirty_textinput = false;

// ADD LISTENERS
var input = document.getElementById("msg-input");
input.addEventListener("input", function () {
    window.dirty_textinput = true;
}, false);
input.addEventListener("keydown", function (e) {
    if (e.key !== "Backspace" && e.keyCode !== 8) return;
    var start = this.selectionStart;
    var end = this.selectionEnd;
    var val = this.value;
    if (start !== end) { // delete selected range
    this.value = val.slice(0, start) + val.slice(end);
    this.setSelectionRange(start, start);
    } else if (start > 0) { // delete previous character
    this.value = val.slice(0, start - 1) + val.slice(end);
    this.setSelectionRange(start - 1, start - 1);
    }
    window.dirty_textinput = true;
    e.preventDefault();
}, false);

// CUSTOMIZE MODULE
var Module = {
    print: (function() {
    var element = document.getElementById("output");
    if (element) element.textContent = ''; // clear previous content
    return function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        console.log(text);
        if (element) {
        element.textContent += text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
        }
    };
    })(),
    canvas: (function() {
    var canvas = document.getElementById("canvas");
    return canvas;
    })()
};
