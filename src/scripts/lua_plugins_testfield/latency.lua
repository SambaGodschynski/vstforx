-- globals
blockSize = 1
latency = 44100
bff = {}
bffSize = blockSize + latency
wc = 0
rc = 0

function allocBuffer()
    bffSize = blockSize + latency
    for i = 1, bffSize, 1 do
        bff[i]=0
    end
    wc = latency
end

function lcInit()
    frxSetModuleLatency(latency)
    allocBuffer()
end

function lcSetBlockSize(x)
    blockSize = x
    allocBuffer()
end

function writeIn(data, numSamples)
    for i = 1, numSamples, 1 do
        bff[wc] = data[i]
        wc = wc + 1
        if wc > bffSize then
            wc = 1
        end
    end
end

function readOut(numSamples)
    data = {}
    for i = 1, numSamples, 1 do
        data[i] = bff[rc]
        rc = rc + 1
        if rc > bffSize then
            rc = 1
        end
    end
    return data
end

function lcProcess(numSamples)
    l, r = frxGetFramesFromInput(1)
    writeIn(l, numSamples)
    out = readOut(numSamples)
    frxSetFramesToOutput(1, out, out)
end
