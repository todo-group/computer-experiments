# computer-experiments

* lecture: includes slides for lecture "Computer Experiments"
* exercise: includes sample programs for exercise "Computer Experiments"

## Documentation

* [Beamer to Quarto/Reveal.js conversion design](docs/beamer-to-quarto-design.md)

## Beamer to Quarto conversion

The converter requires Python 3.10 or later. Install the test dependency and
convert one deck with:

```sh
python3 -m pip install -r requirements.txt
python3 tools/beamer_to_quarto.py lecture/lecture-1-1.tex --output-dir quarto
```

To audit all master documents without writing output, run:

```sh
python3 tools/beamer_to_quarto.py lecture --all-masters --check \
  --report conversion-report.json
```

Use `--strict` in CI to fail when an input or image is missing. Run
`python3 tools/beamer_to_quarto.py --help` for all options.
