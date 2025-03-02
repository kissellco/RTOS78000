"""
Author: Ben Janis
Date: 2025

This source file is part of an example system for MITRE's 2025 Embedded System CTF
(eCTF). This code is being provided only for educational purposes for the 2025 MITRE
eCTF competition, and may not meet MITRE standards for quality. Use this code at your
own risk!

Copyright: Copyright (c) 2025 The MITRE Corporation
"""

import argparse
import base64
import json
import math
import random
import time
from collections import namedtuple

from loguru import logger
from tqdm import tqdm

from ectf25.utils import Encoder
from ectf25.utils.decoder import DecoderIntf

Frame = namedtuple("Frame", ["channel", "data", "timestamp"])


def test_encoder(args):
    """Test the encoder by generating `args.test_size` frames and encoding

    Optionally, store the frames to a json file to be used with the `test_decoder` function
    """
    encoder = Encoder(args.secrets.read())
    nframes = math.ceil(args.test_size / args.frame_size)
    logger.info(f"Generating frames ({nframes:,} {args.frame_size}B frames)...")
    frames = [
        Frame(
            random.choice(args.channels),  # pick random channel
            random.randbytes(args.frame_size),  # generate random frame
            x * 1000,  # generate microsecond timestamp
        )
        for x in range(nframes)
    ]

    logger.info("Running stress test...")
    encoded_frames = []
    start = time.perf_counter()
    for frame in tqdm(frames):
        try:
            encoded_frames.append(
                Frame(frame.channel, encoder.encode(*frame), frame.timestamp)
            )
        except Exception as e:
            logger.error(f"Errored on frame {frame}!")
            raise e
    total = time.perf_counter() - start

    if args.dump is not None:
        logger.info("Dumping encoded frames...")
        json.dump(
            [
                [
                    frame.channel,  # channel
                    base64.b64encode(frame.data).decode(),  # encoded frame
                    frame.timestamp,  # timestamp
                ]
                for frame in tqdm(encoded_frames)
            ],
            args.dump,
        )

    kb_threshold = args.threshold / 1000
    kb_throughput = args.test_size / total / 1000
    if kb_throughput < kb_threshold:
        logger.error(
            f"Throughput too slow! {kb_throughput:,.2f} KBps < {kb_threshold:,.2f} KBps"
        )
        exit(-1)
    else:
        logger.success(
            "Throughput sufficient:"
            f" {kb_throughput:,.2f} KBps > {kb_threshold:,.2f} KBps"
        )


def test_decoder(args):
    """Test the decoder by passing the frames generated by `test_encoder` to the decoder"""
    decoder = DecoderIntf(args.port)
    logger.info("Loading encoded frames...")
    frames = [
        Frame(
            frame[0],  # channel
            base64.b64decode(frame[1]),  # encoded frame
            frame[2],  # timestamp
        )
        for frame in tqdm(json.load(args.frames))
    ]

    logger.info("Running stress test...")
    total_frame_len = 0
    start = time.perf_counter()
    for frame in tqdm(frames):
        try:
            frame_data = decoder.decode(frame.data)
            total_frame_len += len(frame_data)
        except Exception as e:
            logger.error(f"Errored on frame {frame}!")
            raise e
    total = time.perf_counter() - start

    # Check threshold
    kb_threshold = args.threshold / 1000
    kb_throughput = total_frame_len / total / 1000
    if kb_throughput < kb_threshold:
        logger.error(
            f"Throughput too slow! {kb_throughput:,.2f} KBps < {kb_threshold:,.2f} KBps"
        )
        exit(-1)
    else:
        logger.success(
            "Throughput sufficient:"
            f" {kb_throughput:,.2f} KBps > {kb_threshold:,.2f} KBps"
        )


def parse_args():
    parser = argparse.ArgumentParser(prog="ectf25.dev.stress_test")
    parser.add_argument(
        "--frame-size", "-f", default=64, type=int, help="Size of frame"
    )
    parser.add_argument(
        "--test-size", "-t", default=100000000, type=int, help="Bytes to process"
    )
    parser.add_argument(
        "--channels",
        "-c",
        nargs="+",
        type=int,
        default=[0, 1, 2, 3],
        help="Channels to randomly chose from (NOTE: 0 is broadcast)",
    )

    subparsers = parser.add_subparsers(required=True)

    encode_parser = subparsers.add_parser("encode", help="Test the encoder")
    encode_parser.set_defaults(tester=test_encoder)
    encode_parser.set_defaults(threshold=64_000.0)
    encode_parser.add_argument(
        "secrets", type=argparse.FileType("rb"), help="Path to the secrets file"
    )
    encode_parser.add_argument(
        "--dump",
        type=argparse.FileType("w"),
        default=None,
        help="Filename of the encoded frames",
    )

    decode_parser = subparsers.add_parser("decode", help="Test the decoder")
    decode_parser.set_defaults(tester=test_decoder)
    decode_parser.set_defaults(threshold=640.0)
    decode_parser.add_argument(
        "port",
        help="Serial port to the Decoder (See https://rules.ectf.mitre.org/2025/getting_started/boot_reference for platform-specific instructions)",
    )
    decode_parser.add_argument(
        "frames",
        type=argparse.FileType("r"),
        help="JSON list of base64-encoded frames (can be created by encoder test)",
    )

    return parser.parse_args()


def main():
    args = parse_args()
    args.tester(args)


if __name__ == "__main__":
    main()
