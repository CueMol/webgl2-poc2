import { useCallback, useRef, useState } from "react";

function makeModif(event) {
    let modif = event.buttons;
    if (event.ctrlKey) {
        modif += 32;
    }
    if (event.shiftKey) {
        modif += 64;
    }
    return modif;
}

const useMouseEvent = () => {
    const [mouseEvent, setMouseEvent] = useState({
        event: "",
        clientX: 0,
        clientY: 0,
        screenX: 0,
        screenY: 0,
        modif: 0,
    });
    
    const handleMouseMove = useCallback(
        (e) =>
        setMouseEvent({
            event: "move",
            clientX: e.clientX,
            clientY: e.clientY,
            screenX: e.screenX,
            screenY: e.screenY,
            modif: makeModif(e),
        }),
        []
    );
    const handleMouseDown = useCallback(
        (e) =>
        setMouseEvent({
            event: "down",
            clientX: e.clientX,
            clientY: e.clientY,
            screenX: e.screenX,
            screenY: e.screenY,
            modif: makeModif(e),
        }),
        []
    );
    const handleMouseUp = useCallback(
        (e) =>
        setMouseEvent({
            event: "up",
            clientX: e.clientX,
            clientY: e.clientY,
            screenX: e.screenX,
            screenY: e.screenY,
            modif: makeModif(e),
        }),
        []
    );

    const ref = useRef();

    const callbackRef = useCallback(
        (node) => {
            if (ref.current) {
                ref.current.removeEventListener("mousemove", handleMouseMove);
                ref.current.removeEventListener("mousedown", handleMouseDown);
                ref.current.removeEventListener("mouseup", handleMouseUp);
            }

            ref.current = node;

            if (ref.current) {
                ref.current.addEventListener("mousemove", handleMouseMove);
                ref.current.addEventListener("mousedown", handleMouseDown);
                ref.current.addEventListener("mouseup", handleMouseUp);
            }
        },
        [handleMouseMove, handleMouseDown, handleMouseUp]
    );

    return [callbackRef, mouseEvent];
};

export default useMouseEvent;
