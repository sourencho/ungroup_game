# ungroup

## Intro

TODO

## Building and Running
**Linux**
1. `Clone this repo`
2. `Install SFML-devel package`
3. `Change directory to source directory, currently ./ungroup_sfml/ungroup_sfml`
4. `run make`
5. `./ungroup_client`

## Mechanics

TODO

## Player

**Inputs**

- `Point`
  - *Action*
    - Click
  - *Target*
    - Point on map or Unit
  - *Effect*
    - `Direction Target` <- `Target.Direction`
  - *Contraints*
    - Target `Group` must be within visible range

- `Merge`
  - *Action*
    - Right Click
  - *Target*
    - Any `Group`
  - *Effect*
    - `Merge Target` <- `Target.Group`
  - *Contraints*
    - Target `Group` must be within visible range

- `Ungroup`
  - *Action*
    - Esc
  - *Target*
    - None
  - *Effect*
    - Un-merge from `Group`
  - *Contraints*
    - `size(Group.Members) > 1`


**Properties**

- `Group`
  - `Group` Player belongs to.
- `Direction Target`
  - Current vote for direction to move `Group`
- `Merge Target`
  - Current vote for which `Group` to merge with
- `Supply`
  - Amount of each `Resource` in possession
- `Goal`
  - Amount of each `Resource` in `Supply` needed to win

## Units

### Group

**Actions**

- `Move`
  - Let `S` be the largest set of `Player`s with the equal `Direction Target`
  - `Direction` <- `S.Direction Target`
  - `Velocity` <- `size(S)`
- `Attack`/`Mine`
  - Strike simultaneously in `Direction Target` of each `Player` in `Members`
- `Merge`
  - Merge with another `Group`
    - If majority of `Members` in both `Group`s have `Merge Target` equal to eachother
    - If `Merge Target` is within merge range


**Properties**

- `Position`
  - Coordinate on map
- `Members`
  - Players belonging to group
- `Velocity`
  - Velocity
- `Direction`
  - Direction of movement

### Mine

TODO: Can a mine have more than one type of resource?

**Properties**

- `Quantity`
  - Remaining quantity of `Resource`
- `Type`
  - Type of `Resource` contained
