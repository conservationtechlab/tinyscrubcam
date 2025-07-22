import json
import os
import random
from pathlib import Path
from PIL import Image

# === CONFIG ===
species_dirs = ["cougar", "bobcat", "coyote"]
base_dir = Path("/home/conor/Downloads/Images/Species_Images")
confidence_threshold = 0.2
split_ratio = 0.8  # 80% training, 20% testing

# === FUNCTION ===
def process_species_dir(species):
    folder = base_dir / species
    json_path = folder / f"detections_{species}.json"

    if not json_path.exists():
        print(f"❌ Expected file not found: {json_path}")
        return

    with open(json_path, 'r') as f:
        md_data = json.load(f)

    entries = []
    for img_entry in md_data["images"]:
        full_path = img_entry["file"]
        filename = os.path.basename(full_path)
        image_path = folder / filename

        if not image_path.exists():
            print(f"⚠️ Missing image: {image_path}")
            continue

        try:
            with Image.open(image_path) as im:
                width, height = im.size
        except Exception as e:
            print(f"❌ Error opening image {image_path}: {e}")
            continue

        boxes = []
        for det in img_entry["detections"]:
            if det["conf"] < confidence_threshold:
                continue

            rel_x, rel_y, rel_w, rel_h = det["bbox"]
            abs_x = int(rel_x * width)
            abs_y = int(rel_y * height)
            abs_w = int(rel_w * width)
            abs_h = int(rel_h * height)

            boxes.append({
                "label": species,
                "x": abs_x,
                "y": abs_y,
                "width": abs_w,
                "height": abs_h,
                "confidence": det["conf"]  # Added confidence here
            })

        if boxes:
            entries.append({
                "path": filename,
                "boundingBoxes": boxes
            })

    # 80/20 split
    random.shuffle(entries)
    split_index = int(len(entries) * split_ratio)
    for i, entry in enumerate(entries):
        entry["category"] = "training" if i < split_index else "testing"

    # Write CSV file with confidence
    csv_path = folder / "bounding_boxes.csv"
    with open(csv_path, 'w') as csvfile:
        csvfile.write("filename,xmin,ymin,xmax,ymax,label,confidence,category\n")
        for entry in entries:
            filename = entry["path"]
            category = entry["category"]
            for box in entry["boundingBoxes"]:
                xmin = box["x"]
                ymin = box["y"]
                xmax = box["x"] + box["width"]
                ymax = box["y"] + box["height"]
                label = box["label"]
                confidence = box["confidence"]
                csvfile.write(f"{filename},{xmin},{ymin},{xmax},{ymax},{label},{confidence},{category}\n")

    print(f"✅ Created CSV: {csv_path}")

# === MAIN ===
if __name__ == "__main__":
    for species in species_dirs:
        process_species_dir(species)


