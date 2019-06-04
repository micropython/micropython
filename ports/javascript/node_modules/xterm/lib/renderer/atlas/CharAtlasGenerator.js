"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Platform_1 = require("../../common/Platform");
var Types_1 = require("./Types");
function generateStaticCharAtlasTexture(context, canvasFactory, config) {
    var cellWidth = config.scaledCharWidth + Types_1.CHAR_ATLAS_CELL_SPACING;
    var cellHeight = config.scaledCharHeight + Types_1.CHAR_ATLAS_CELL_SPACING;
    var canvas = canvasFactory(255 * cellWidth, (2 + 16 + 16) * cellHeight);
    var ctx = canvas.getContext('2d', { alpha: config.allowTransparency });
    ctx.fillStyle = config.colors.background.css;
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.save();
    ctx.fillStyle = config.colors.foreground.css;
    ctx.font = getFont(config.fontWeight, config);
    ctx.textBaseline = 'middle';
    for (var i = 0; i < 256; i++) {
        ctx.save();
        ctx.beginPath();
        ctx.rect(i * cellWidth, 0, cellWidth, cellHeight);
        ctx.clip();
        ctx.fillText(String.fromCharCode(i), i * cellWidth, cellHeight / 2);
        ctx.restore();
    }
    ctx.save();
    ctx.font = getFont(config.fontWeightBold, config);
    for (var i = 0; i < 256; i++) {
        ctx.save();
        ctx.beginPath();
        ctx.rect(i * cellWidth, cellHeight, cellWidth, cellHeight);
        ctx.clip();
        ctx.fillText(String.fromCharCode(i), i * cellWidth, cellHeight * 1.5);
        ctx.restore();
    }
    ctx.restore();
    ctx.font = getFont(config.fontWeight, config);
    for (var colorIndex = 0; colorIndex < 16; colorIndex++) {
        var y = (colorIndex + 2) * cellHeight;
        for (var i = 0; i < 256; i++) {
            ctx.save();
            ctx.beginPath();
            ctx.rect(i * cellWidth, y, cellWidth, cellHeight);
            ctx.clip();
            ctx.fillStyle = config.colors.ansi[colorIndex].css;
            ctx.fillText(String.fromCharCode(i), i * cellWidth, y + cellHeight / 2);
            ctx.restore();
        }
    }
    ctx.font = getFont(config.fontWeightBold, config);
    for (var colorIndex = 0; colorIndex < 16; colorIndex++) {
        var y = (colorIndex + 2 + 16) * cellHeight;
        for (var i = 0; i < 256; i++) {
            ctx.save();
            ctx.beginPath();
            ctx.rect(i * cellWidth, y, cellWidth, cellHeight);
            ctx.clip();
            ctx.fillStyle = config.colors.ansi[colorIndex].css;
            ctx.fillText(String.fromCharCode(i), i * cellWidth, y + cellHeight / 2);
            ctx.restore();
        }
    }
    ctx.restore();
    if (!('createImageBitmap' in context) || Platform_1.isFirefox || Platform_1.isSafari) {
        return canvas;
    }
    var charAtlasImageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
    clearColor(charAtlasImageData, config.colors.background);
    return context.createImageBitmap(charAtlasImageData);
}
exports.generateStaticCharAtlasTexture = generateStaticCharAtlasTexture;
function clearColor(imageData, color) {
    var isEmpty = true;
    var r = color.rgba >>> 24;
    var g = color.rgba >>> 16 & 0xFF;
    var b = color.rgba >>> 8 & 0xFF;
    for (var offset = 0; offset < imageData.data.length; offset += 4) {
        if (imageData.data[offset] === r &&
            imageData.data[offset + 1] === g &&
            imageData.data[offset + 2] === b) {
            imageData.data[offset + 3] = 0;
        }
        else {
            isEmpty = false;
        }
    }
    return isEmpty;
}
exports.clearColor = clearColor;
function getFont(fontWeight, config) {
    return fontWeight + " " + config.fontSize * config.devicePixelRatio + "px " + config.fontFamily;
}
//# sourceMappingURL=CharAtlasGenerator.js.map