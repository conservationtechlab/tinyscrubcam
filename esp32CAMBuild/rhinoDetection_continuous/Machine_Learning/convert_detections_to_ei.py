import json
import os
import shutil

def convert_cougar_detections_to_ei(megadetector_json_path, images_folder, output_folder):
    # Load MegaDetector JSON
    with open(megadetector_json_path, 'r') as f:
        md_data = json.load(f)

    # Prepare the Edge Impulse bounding boxes structure
    bounding_boxes_json = {
        "version": 1,
        "type": "bounding-box-labels",
        "boundingBoxes": {}
    }

    # Make sure output folder exists
    os.makedirs(output_folder, exist_ok=True)

    # Iterate over each detection item in MegaDetector JSON
    for item in md_data.get("images", []):
        image_name = item.get("file") or item.get("file_name") or item.get("image") or None
        if not image_name:
            continue

        # Filter detections for label "animal" and convert to "cougar"
        boxes_for_image = []
        for det in item.get("detections", []):
            label = det.get("category", "")
            if label == "animal":  # Change "animal" to "cougar"
                # Bounding box coords in MegaDetector are normalized [x,y,width,height]
                # where x,y is the top-left corner in normalized coords (0-1)
                bbox = det.get("bbox", None)
                if bbox and len(bbox) == 4:
                    x_norm, y_norm, w_norm, h_norm = bbox
                    # Load image size to convert normalized to pixel coords
                    img_path = os.path.join(images_folder, image_name)
                    if not os.path.isfile(img_path):
                        print(f"Warning: image {img_path} not found, skipping")
                        continue

                    from PIL import Image
                    with Image.open(img_path) as img:
                        img_width, img_height = img.size

                    # Convert normalized to absolute pixel values (integers)
                    x = int(x_norm * img_width)
                    y = int(y_norm * img_height)
                    width = int(w_norm * img_width)
                    height = int(h_norm * img_height)

                    boxes_for_image.append({
                        "label": "cougar",
                        "x": x,
                        "y": y,
                        "width": width,
                        "height": height
                    })

        if boxes_for_image:
            bounding_boxes_json["boundingBoxes"][image_name] = boxes_for_image
            # Copy image to output folder
            src_path = os.path.join(images_folder, image_name)
            dst_path = os.path.join(output_folder, image_name)
            shutil.copy2(src_path, dst_path)

    # Save bounding-boxes.labels file in output folder
    output_json_path = os.path.join(output_folder, "bounding-boxes.labels")
    with open(output_json_path, 'w') as f_out:
        json.dump(bounding_boxes_json, f_out, indent=2)

    print(f"Conversion complete. EI dataset created at: {output_folder}")

# Example usage
if __name__ == "__main__":
    megadetector_json = "/home/username/Downloads/detections_cougar.json"
    images_dir = "/home/username/Downloads/Images/Species_Images/cougar"
    ei_output_dir = "/home/username/Downloads/Images/Species_Images/cougar/ei_dataset"

    convert_cougar_detections_to_ei(megadetector_json, images_dir, ei_output_dir)

