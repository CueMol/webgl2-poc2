import React from "react";
import "./MolView.css";

export default function MolView({id}) {
  return (
      <div id="placeholder">
          <canvas id={id}></canvas>
      </div>
  );
}
