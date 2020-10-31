const fs = require("fs");

function cost(patterns, dataset) {
  let falsePositives = 0;
  let falseNegatives = 0;

  dataset.forEach((row, i) =>
    row.forEach((item) => {
      if (
        !patterns.find((p) => p.trIds.includes(i) && p.items.includes(item))
      ) {
        falseNegatives++;
      }
    })
  );

  patterns.forEach((pattern) =>
    pattern.trIds.forEach((trId) =>
      pattern.items.forEach((item) => {
        if (!dataset[trId].includes(item)) {
          falsePositives++;
        }
      })
    )
  );

  const complexity = patterns.reduce(
    (prev, pattern) => prev + pattern.trIds.length + pattern.items.length,
    0
  );

  return falsePositives + falseNegatives + complexity * 0.5;
}

function extendCore(core, extensionList, patterns, dataset, eRow, eCol) {
  let addedItem = true;

  while (addedItem) {
    dataset.forEach((row, i) => {
      if (!core.trIds.includes(i)) {
        const candidate = {
          trIds: [...core.trIds, i],
          items: core.items,
        };
        if (
          cost([...patterns, candidate], dataset) <=
          cost([...patterns, core], dataset)
        ) {
          core = candidate;
        }
      }
    });

    addedItem = false;
    while (extensionList.length > 0) {
      const e = extensionList.shift();
      const candidate = {
        trIds: core.trIds,
        items: [...core.items, e],
      };
      if (
        cost([...patterns, candidate], dataset) <=
        cost([...patterns, core], dataset)
      ) {
        core = candidate;
        addedItem = true;
        break;
      }
    }
  }

  return core;
}

function sortItemsInDb(residualDataset) {
  const frequencies = new Map();
  residualDataset.forEach((row) =>
    row.forEach((item) => {
      if (frequencies.has(item)) {
        frequencies.set(item, frequencies.get(item) + 1);
      } else {
        frequencies.set(item, 1);
      }
    })
  );
  return Array.from(frequencies.keys()).sort(
    (a, b) => frequencies.get(b) - frequencies.get(a)
  );
}

function findCore(residualDataset, patterns, dataset) {
  const extensionList = [];
  const [s1, ...s] = sortItemsInDb(residualDataset);

  let core = {
    trIds: new Array(residualDataset.length)
      .fill()
      .map((k, i) => i)
      .filter((i) => residualDataset[i].includes(s1)),
    items: [s1],
  };

  s.forEach((sh) => {
    const candidate = {
      trIds: core.trIds.filter((i) => residualDataset[i].includes(sh)),
      items: [...core.items, sh],
    };

    if (
      cost([...patterns, candidate], dataset) <=
      cost([...patterns, core], dataset)
    ) {
      core = candidate;
    } else {
      extensionList.push(sh);
    }
  });

  return [core, extensionList];
}

function panda(maxK, dataset, eRow, eCol) {
  const patterns = [];
  let residualDataset = dataset.map((d) => [...d]);

  for (let k = 0; k < maxK; k++) {
    const [core, extensionList] = findCore(residualDataset, patterns, dataset);
    const extendedCore = extendCore(
      core,
      extensionList,
      patterns,
      dataset,
      eRow,
      eCol
    );

    if (cost(patterns, dataset) < cost([...patterns, extendedCore], dataset)) {
      break;
    }

    patterns.push(extendedCore);
    residualDataset = residualDataset.map((row, trId) =>
      row.filter(
        (item) =>
          !(
            extendedCore.trIds.includes(trId) &&
            extendedCore.items.includes(item)
          )
      )
    );
  }

  return patterns;
}

const fileLines = fs.readFileSync(process.argv[2], "utf-8").split("\n");
const dataset = fileLines.map((line) =>
  line.split(" ").map((i) => parseInt(i))
);

const patterns = panda(3, dataset, 1.0, 1.0);
patterns.forEach((p) => console.log(p.items, `(${p.trIds.length})`));
console.log("cost", cost(patterns, dataset));
