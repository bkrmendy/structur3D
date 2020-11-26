CREATE TABLE IF NOT EXISTS radius (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    magnitude REAL NOT NULL CHECK (magnitude >= 0)
);

CREATE TABLE IF NOT EXISTS coord (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    x real NOT NULL,
    y real NOT NULL,
    z real NOT NULL
);

CREATE TABLE IF NOT EXISTS setoperationtype (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    type INTEGER NOT NULL CHECK (type = 0 or type = 1 or type = 2)
);

CREATE TABLE IF NOT EXISTS edge (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    entity_to TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS document (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    type integer NOT NULL,
    document TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS name (
    deleted INTEGER NOT NULL DEFAULT 0,
    timestamp UNSIGNED BIG INT NOT NULL,
    entity TEXT NOT NULL,
    name TEXT NOT NULL
);

