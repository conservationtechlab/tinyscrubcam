import json
import os
from pathlib import Path
from PIL import Image

# === CONFIG ===
species_dirs = ["cougar", "bobcat", "coyote"]
base_dir = Path("/home/conor/Downloads/Images/Species_Images")
confidence_threshold = 0.2

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

        for det in img_entry["detections"]:
            if det["conf"] < confidence_threshold:
                continue

            rel_x, rel_y, rel_w, rel_h = det["bbox"]
            abs_x = int(rel_x * width)
            abs_y = int(rel_y * height)
            abs_w = int(rel_w * width)
            abs_h = int(rel_h * height)

            xmin = abs_x
            ymin = abs_y
            xmax = abs_x + abs_w
            ymax = abs_y + abs_h

            XMin = max(0.0, min(1.0, xmin / width))
            XMax = max(0.0, min(1.0, xmax / width))
            YMin = max(0.0, min(1.0, ymin / height))
            YMax = max(0.0, min(1.0, ymax / height))

            entries.append({
                "ImageID": filename,
                "LabelName": species,
                "Confidence": det["conf"],
                "XMin": XMin,
                "XMax": XMax,
                "YMin": YMin,
                "YMax": YMax,
            })

    # Write Open Images CSV per species
    csv_path = folder / f"{species}_annotations.csv"
    with open(csv_path, 'w') as csvfile:
        csvfile.write("ImageID,LabelName,Confidence,XMin,XMax,YMin,YMax\n")
        for entry in entries:
            csvfile.write(
                f"{entry['ImageID']},{entry['LabelName']},{entry['Confidence']:.3f},"
                f"{entry['XMin']:.6f},{entry['XMax']:.6f},{entry['YMin']:.6f},{entry['YMax']:.6f}\n"
            )

    print(f"✅ Created CSV for {species}: {csv_path}")

    # Create label_map.json for the species
    label_map = {species: species}
    label_map_path = folder / "class-descriptions.csv"
    with open(label_map_path, "w") as f:
        json.dump(label_map, f, indent=2)
    print(f"✅ Created label map for {species}: {label_map_path}")

if __name__ == "__main__":
    for species in species_dirs:
        process_species_dir(species)





