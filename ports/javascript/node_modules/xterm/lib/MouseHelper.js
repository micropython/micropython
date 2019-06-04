"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var MouseHelper = (function () {
    function MouseHelper(_renderCoordinator) {
        this._renderCoordinator = _renderCoordinator;
    }
    MouseHelper.getCoordsRelativeToElement = function (event, element) {
        var rect = element.getBoundingClientRect();
        return [event.clientX - rect.left, event.clientY - rect.top];
    };
    MouseHelper.prototype.getCoords = function (event, element, charMeasure, colCount, rowCount, isSelection) {
        if (!charMeasure.width || !charMeasure.height) {
            return null;
        }
        var coords = MouseHelper.getCoordsRelativeToElement(event, element);
        if (!coords) {
            return null;
        }
        coords[0] = Math.ceil((coords[0] + (isSelection ? this._renderCoordinator.dimensions.actualCellWidth / 2 : 0)) / this._renderCoordinator.dimensions.actualCellWidth);
        coords[1] = Math.ceil(coords[1] / this._renderCoordinator.dimensions.actualCellHeight);
        coords[0] = Math.min(Math.max(coords[0], 1), colCount + (isSelection ? 1 : 0));
        coords[1] = Math.min(Math.max(coords[1], 1), rowCount);
        return coords;
    };
    MouseHelper.prototype.getRawByteCoords = function (event, element, charMeasure, colCount, rowCount) {
        var coords = this.getCoords(event, element, charMeasure, colCount, rowCount);
        var x = coords[0];
        var y = coords[1];
        x += 32;
        y += 32;
        return { x: x, y: y };
    };
    return MouseHelper;
}());
exports.MouseHelper = MouseHelper;
//# sourceMappingURL=MouseHelper.js.map